# IBD Stall Fix — Orphan-Accumulation Watchdog

## Summary

The upstream Alias daemon (Bitcoin Core pre-headers-first lineage,
inherited from SpectreCoin/ShadowCoin) had a latent **Initial Block
Download (IBD) stall bug** that could lock sync indefinitely without
any error log, crash, or visible misbehaviour. This document records
the bug, how it manifests, the root cause, and the fix we shipped.

## Symptom

On a wallet whose chain tip is far behind the network (in our repro:
~660,000 blocks behind), the daemon would:

- Stay connected to many peers (10–15) with healthy `pingtime` and
  zero `banscore`
- Continuously receive `mblk` messages (multi-block payloads) from
  peers — log shows `Received mblk 1` every few seconds
- Process every received block as an **orphan** (parent not in local
  chain) — `ProcessBlock: ORPHAN BLOCK 1...2...3...n` accumulating
  without bound
- **Never accept a single block** — no `ProcessBlock: ACCEPTED` lines,
  no `SetBestChain: new best` lines
- Stay in this state forever; restart does not help

From `debug.log` of a stalled daemon:

```
ProcessBlock: ORPHAN BLOCK 643, prev=7064d6ed35b0...
Received mblk 1
ProcessBlock: ORPHAN BLOCK 644, prev=527b6ffaeed5...
Received mblk 1
ProcessBlock: ORPHAN BLOCK 645, prev=...
```

`getpeerinfo` shows peers with `chainheight` near the real network
tip, but `getinfo.blocks` never advances.

## Root cause

The daemon has a self-healing mechanism in `SendMessages` (`main.cpp`
~line 6776): if no `mblk` is received for `MBLK_RECEIVE_TIMEOUT` (60 s)
while sync is needed, it sends a fresh `getblocks` to a different peer.

```cpp
// Original retry condition (paraphrased):
if (nNodeMode == NT_FULL
    && nTimeLastMblkRecv > 0
    && pto->nChainHeight - nBestHeight > 256
    && nTimeNow - nTimeLastMblkRecv > MBLK_RECEIVE_TIMEOUT)
{
    pto->PushGetBlocks(pindexBest, uint256(0));
    nTimeLastMblkRecv = nTimeNow; // reset
}
```

The bug: the retry trigger checks whether **any `mblk` was received**,
not whether **any block was accepted**. In the orphan-stall state,
peers ARE sending `mblk` messages — they're just tip-block
announcements that arrive as orphans because the local chain is far
behind. `nTimeLastMblkRecv` keeps getting refreshed by these orphan
deliveries, so the 60 s timeout **never fires**, and the daemon never
re-asks for the in-order range it actually needs.

Meanwhile the orphan-resolution path (`AskFor(WantedByOrphan(...))`
inside `ProcessBlock`) does try to fetch missing parents, but it asks
the same peers that announced the orphans — peers whose chain forked
hundreds of thousands of blocks ago from our tip, so they don't have
the parents at our height. The orphan map grows without bound and the
chain never moves.

## Fix

Add a **second watchdog** that tracks "time since last block accepted"
rather than "time since last mblk received". When that crosses
`BLOCK_ACCEPT_STALL_TIMEOUT` (5 min) AND we're still well behind a
peer, force a fresh `getblocks` to that peer — same recovery action as
the original timeout, but triggered by a different (and correct)
condition.

### Files changed

| File | Change |
|---|---|
| `src/state.h` | Added `BLOCK_ACCEPT_STALL_TIMEOUT` (5 min) + `extern int64_t nTimeLastBlockAccepted` |
| `src/state.cpp` | Defined `int64_t nTimeLastBlockAccepted = 0` |
| `src/main.cpp` (`CBlock::SetBestChain`, just after the `new best` log) | `nTimeLastBlockAccepted = GetTime();` — stamped only on real chain advance |
| `src/main.cpp` (mblk message handler) | On first `mblk` of session, seed `nTimeLastBlockAccepted` so the watchdog clock starts ticking |
| `src/main.cpp` (`SendMessages` retry block) | Replaced single `fMblkSilent` check with `fMblkSilent OR fAcceptStalled`, where `fAcceptStalled = now - nTimeLastBlockAccepted > BLOCK_ACCEPT_STALL_TIMEOUT` |

When the new watchdog fires, the daemon logs:

```
Sync stall (orphans accumulating), getblocks to <peer>:<port>, from <height>
```

distinguishing it from the original `Sync timeout, getblocks to ...`
message so operators can tell which condition triggered the recovery.

### Why 5 minutes

- Long enough to distinguish a real stall from a legitimate validation
  pause (large block, slow disk, big reorg)
- Short enough that a user staring at a frozen sync icon doesn't wait
  more than a few minutes before recovery starts
- Each retry switches peer in the normal round-robin of `SendMessages`,
  so successive watchdog firings naturally distribute load across the
  peer set

## Verified outcome

On a wallet that had been stuck at height 2,823,135 for hours with
peers at 3,485,783 (~660K-block gap), the patched daemon:

1. Detects the orphan-stall after 5 min via the new watchdog
2. Logs `Sync stall (orphans accumulating), getblocks to <peer>, from 2823135`
3. Sends fresh `getblocks` from current height
4. Begins receiving blocks at the correct height and connecting them

Note that the watchdog cannot recover from a **chain divergence**
(where the local chain is on a fork the network has abandoned). If
peers return their fork's blocks instead of extending ours, the orphan
state will reappear regardless. That case requires either rolling back
via a fresh bootstrap or re-IBD from genesis — neither of which the
daemon can do autonomously.

## Companion fix in the Electron shell

The Electron host (`alias-wallet-desktop`) also added an
**auto-respawn watchdog** in `src/main/main.js`: when `aliaswalletd`
exits unexpectedly (not via our own `gracefulStopDaemon` and not
during app shutdown), the parent process kills any orphan `tor.exe`
and respawns the daemon after 3 s. This handles the secondary
class of stalls where the daemon crashes outright (no clean shutdown
log, no Windows Application Error event captured) — most often during
long IBD sessions. Sync resumes without user intervention.

## References

- `src/main.cpp` — `SendMessages` and `ProcessBlock` paths
- `src/state.h`, `src/state.cpp` — watchdog globals
- Commit message on the daemon repo: "add IBD stall watchdog ..."
- Commit message on the desktop repo: "harden startup flow ... auto-respawn watchdog ..."
