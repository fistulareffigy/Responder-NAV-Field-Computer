# Release binaries

Versioned binaries and `SHA256SUMS.txt` are published as GitHub Release assets
for the matching source tag. Use the factory image for a fresh installation;
the application-only image is for an existing matching bootloader and
partition table.

The local public-release staging workspace keeps the generated v0.71 Beta files
in its sibling `firmware/` directory. Binary files are intentionally excluded
from normal source commits so a source checkout cannot silently contain a stale
firmware image.
