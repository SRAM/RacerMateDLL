
@echo off
@call poo
del trinerd.zip

pkzip -add -rec -directories trinerd @files.i

@dir trinerd.zip

