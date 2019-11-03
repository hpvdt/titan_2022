#!/usr/bin/env python2

from config import CONFIG
# The purpose of this is to simply decide which bike system to run

if CONFIG == "backup":
    from sparePi import *
else:
    assert CONFIG in ["main", "rear"], "CONFIG must be either main, rear, or backup"
    from bikePi import *
