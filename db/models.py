"""
Generic tables
"""
from __future__ import division, absolute_import, print_function, unicode_literals

from django.db import models

class Tank(models.Model):
    """Seperate control zones"""
    name = models.CharField(max_length=255)

class Sensor(models.Model):
    """Each data source"""
    name = models.CharField(max_length=255)

class SensorTempBuoy(models.Model):
    """Calibration and validation data for check data from Buoy based temperature sensors"""
    sensor = models.ForeignKey(Sensor)
    target_lat = models.FloatField()
    target_lon = models.FloatField()
    max_position_delta = models.FloatField()
    max_temp = models.FloatField()
    min_temp = models.FloatField()
    #TODO: Cal and val data

class SensorLuxScrape(models.Model):
    """Settings to scrape Lux values for"""
    sensor = models.ForeignKey(Sensor)
    target_lat = models.FloatField()
    target_lon = models.FloatField()

class SensorTempTank(models.Model):
    """Calibration and validation for tank based temperature sensors."""
    sensor = models.ForeignKey(Sensor)
    max_temp = models.FloatField()
    min_temp = models.FloatField()

class SensorTempAmbient(models.Model):
    """Calibration and validation for other temperature sensors."""
    sensor = models.ForeignKey(Sensor)
    max_temp = models.FloatField()
    min_temp = models.FloatField()

class SensorLux(models.Model):
    """Calibration and validation for light sensors."""
    sensor = models.ForeignKey(Sensor)
    max_lux = models.FloatField()
    min_lux = models.FloatField()

class Temp(models.Model):
    """Log of valid temperature readings"""
    sensor = models.ForeignKey(Sensor)
    temp = models.FloatField()
    time = models.DateTimeField()

class Lux(models.Model):
    """Log of valid light readings"""
    sensor = models.ForeignKey(Sensor)
    lux = models.FloatField(Sensor)
    time = models.DateTimeField()
    
class Profile(models.Model):
    """The model temperature and lux that each tank is following"""
    tank = models.ForeignKey(Tank)
    time = models.DateTimeField()
    temp = models.FloatField()
    lux = models.FloatField()

class TankTempParams(models.Model):
    """Parameters to build the Profile model for each tank"""
    tank = models.ForeignKey(Tank)
    sensor = models.ForeignKey(Sensor)
    time_offset = models.FloatField()
    value_offset = models.FloatField()
    start_time = models.DateTimeField()
    end_time = models.DateTimeField()

class TankLuxParams(models.Model):
    """Parameters to build the light model for each Profile"""
    tank = models.ForeignKey(Tank)
    sensor = models.ForeignKey(Sensor)
    time_offset = models.FloatField()
    value_offset = models.FloatField()
    start_time = models.DateTimeField()
    end_time = models.DateTimeField()

class Moonlight(models.Model):
    """Moonline observations"""
    sensor = models.ForeignKey(Sensor)
    rise = models.DateTimeField()
    set = models.TimeField()
    percent = models.FloatField()
