## Backitup

Recursively scans and monitors a file path, backing up any changes. Current implementation only supports backing up to a local path.

```
§ ./bin/backitup --help
Usage: backitup [OPTIONS] PATH:
--help                      produce help message
--index FILE (=backitup.db) File index database path.
--storage PATH (=storage)   Path to file storage for backup destination.
```

## Development

### Component Design

* **Index** responsible for storing all the file metadata for backups.
* **Storage** manages the blobs of file data.
* **BackupPath** crawls and watches the filesystem.
* **Engine** integrates the three above components together.

#### Index

Currently only supports an in-memory model. Uses ~0.5 GB of memory and ~180MB of disk for ~1mil files.

Planned implementations:
* SQLite

#### Storage

Currently only supports backing up to local directory.

Planned implementations:
* S3
* SCP/SSH

#### BackupPath

Currently supports FsEvent Api (MacOS).

Planned implementations:
* Linux
* Windows

## Ideas

* Implement retention by deleting hashes
* Ship hashes to remote agent
** Agent can "glob" up hahses
** Consolidate hashes to implement retention
* Probe kernel for file writes
** Only backup what is written to file, not entire file
