"""
Satalite data
=============

:mod:`models` Module
--------------------

.. automodule:: satalite.models

:mod:`functions` Module
--------------------
"""
from __future__ import division, absolute_import, print_function, unicode_literals

from . import models
from db import models as db_models
from datetime import timedelta

def proc_satalite():
    """Process and verify data from satalite"""
    for log in models.LogTempLive.objects.filter(processed = False):
        delta = -15
        for reading in reversed(log.data):
            delta = delta - 30
            temp = db_models.Temp(
                sensor = log.sensor,
                time = log.time + timedelta(minutes = delta),
                temp = convert_rtd(ord(reading))
            )
            temp.save()
        log.processed = True
        log.save()
    #TODO: should we fillin back temps here?

def convert_rtd(value):
    """Convert the ADC reading in to a temperiture"""
    #TODO:rtd function
    return value
