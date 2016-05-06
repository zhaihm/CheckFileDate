# CheckFileDate
A tool to check file modify date.

### Introduction
We want to know when did folder's files modified sometime, but on Windows, only add/delete file/sub-folder change folder's modify time. So I write this tool to easily **list recently modified files**.

### Usage
`CheckFileDate [-n date] path`

* **-n** list files newer than *date*, *date* **must** be in format "2016-05-04 21:30:00"

### Todo
Add -r option to recover files from backup.