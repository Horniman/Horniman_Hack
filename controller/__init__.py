"""
"""
from __future__ import division, absolute_import, print_function, unicode_literals

from . import models
from db import models as db_models

def proc_controller():
    models.LogTempLive.objects.filter(processed = False)

