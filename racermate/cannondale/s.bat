
@echo off
@call poo
del cannondale.zip

pkzip -add -rec -directories cannondale @files.i

@dir cannondale.zip

