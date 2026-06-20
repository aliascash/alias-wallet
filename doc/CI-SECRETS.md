# GitHub Actions secrets & variables

The build workflows (`.github/workflows/build-develop.yml`, `build-master.yml`) read
the values below. Configure them under **Settings → Secrets and variables → Actions**.

All repository references in the workflows are dynamic — they resolve to the repo the
workflow runs in (`${{ github.repository_owner }}` / `${{ github.event.repository.name }}`),
so nothing is hardcoded to a specific fork.

## Secrets

| Secret | Required? | Used by | Value |
|---|---|---|---|
| `GITHUB_TOKEN` | Auto-provided | release upload steps | Provided automatically by GitHub Actions — do **not** create it. |
| `DOCKERHUB_USERNAME` | Only for the `buildDockerImage` job | Docker Hub login and the image push namespace (`<username>/docker-aliaswalletd`) | Your Docker Hub username (not the email). |
| `DOCKERHUB_TOKEN` | Only for the `buildDockerImage` job | Docker Hub login | A Docker Hub access token (Docker Hub → Account settings → Security → New Access Token). |

## Notes

- If `DOCKERHUB_USERNAME` / `DOCKERHUB_TOKEN` are not set, only `buildDockerImage` fails;
  the Linux / Windows / Windows-x86 / macOS / release jobs still succeed.
- `buildDockerImage` also checks out `<owner>/docker-aliaswalletd` (the Docker installer
  repo) under the same owner as this repo. That repo must exist for the job to complete.
- The per-distro upload Dockerfiles (`Docker/*/Dockerfile`) take `ALIAS_USER` /
  `ALIAS_REPOSITORY` build args; CI passes the current repo's owner/name automatically.
  Their in-file defaults are only used for manual `docker build` runs.
