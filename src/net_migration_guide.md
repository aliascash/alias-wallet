# Networking Migration Guide

## Adding fVerified Flag to Modern CNode

### Step 1: Modify particl-core/src/net.h

Add the fVerified flag to the CNode class:

```cpp
class CNode {
    // ... existing members ...
    
    // Alias-specific: Tor verification flag
    std::atomic_bool fVerified{false};  // tor implementation
    
    // ... rest of class ...
};
```

### Step 2: Initialize in Constructor

In `CNode::CNode()` constructor in `particl-core/src/net.cpp`:

```cpp
CNode::CNode(...)
    : // ... existing initializers ...
      fVerified{false}  // Initialize Tor verification flag
{
    // ... rest of constructor ...
}
```

### Step 3: Use in Connection Logic

When establishing Tor connections, set fVerified:

```cpp
// In connection establishment code
if (addr.IsTor() && /* Tor verification successful */) {
    pnode->fVerified.store(true);
}
```

### Step 4: Use Helper Functions

Use the helper functions from `net_node_alias.h`:

```cpp
#include <net_node_alias.h>

// Check if verified
if (AliasNode::IsTorVerified(pnode)) {
    // Handle verified Tor connection
}

// Set verification status
AliasNode::SetTorVerified(pnode, true);
```

## Preserving Tor Integration

### StartTor Function

The `StartTor()` function from `net_alias.cpp` should be called during node initialization:

```cpp
// In init.cpp or similar
if (gArgs.GetBoolArg("-tor", true)) {
    StartTor(nullptr);
}
```

### ThreadOnionSeed

The onion seed thread should be started alongside other network threads:

```cpp
// In StartNode or similar
threadGroup.create_thread(boost::bind(&TraceThread<void (*)()>, "onionseed", &ThreadOnionSeed));
```

## Migration Checklist

- [ ] Add fVerified flag to CNode class
- [ ] Initialize fVerified in constructor
- [ ] Set fVerified when Tor connection is verified
- [ ] Use helper functions for fVerified access
- [ ] Integrate StartTor() into node initialization
- [ ] Start ThreadOnionSeed thread
- [ ] Update all Tor-related connection logic
- [ ] Test Tor connections work correctly
- [ ] Verify fVerified flag is set/checked properly

