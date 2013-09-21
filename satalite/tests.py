""""""
from __future__ import division, absolute_import, print_function, unicode_literals

def test_proc_satalite():
    """
    >>> from . import models, proc_satalite
    >>> from db import models as db_models
    >>> import datetime
    >>> sensor = db_models.Sensor(name="Test Sat")
    >>> sensor.save()
    >>> test = models.LogTempLive(
    ...     time = datetime.datetime(2012, 12, 12, 12, 0),
    ...     data = chr(1) * 48,
    ...     sensor = sensor
    ... )
    >>> test.save()
    >>> proc_satalite()
    >>> test = models.LogTempLive.objects.filter(sensor=sensor)[0]
    >>> test.processed
    True
    """      

def test_convert_rtd():
    """
    >>> import satalite
    >>> satalite.convert_rtd(0)
    0
    >>> satalite.convert_rtd(7000000)
    7000000
    """
