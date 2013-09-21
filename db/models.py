from django.db import models

# Create your models here.

class Tank(models.Model):
    name = models.CharField(max_length=255)

class Profile(models.Model):
    tank = models.ForeignKey(Tank)

class Sensor(models.Model):
    name = models.CharField(max_length=255)

class SensorTempBuoy(models.Model):
    sensor = models.ForeignKey(Sensor)
    target_lat = models.FloatField()
    target_lon = models.FloatField()
    max_position_delta = models.FloatField()
    max_temp = models.FloatField()
    min_temp = models.FloatField()
    #TODO: Cal and val data

class SensorTempTank(models.Model):
    sensor = models.ForeignKey(Sensor)
    max_temp = models.FloatField()
    min_temp = models.FloatField()

class SensorTempAmbiant(models.Model):
    sensor = models.ForeignKey(Sensor)
    max_temp = models.FloatField()
    min_temp = models.FloatField()

class SensorLux(models.Model):
    sensor = models.ForeignKey(Sensor)
    max_lux = models.FloatField()
    min_lux = models.FloatField()

class Temp(models.Model):
    sensor = models.ForeignKey(Sensor)
    temp = models.FloatField() 
    time = models.DateTimeField()

class Lux(models.Model):
    sensor = models.ForeignKey(Sensor)
    lux = models.FloatField(Sensor) 
    time = models.DateTimeField()
    
class Profile(models.Model):
    tank = models.ForeignKey(Tank)    
    time = models.DateTimeField()
    temp = models.FloatField()
    lux = models.FloatField()

class TankTempParams(models.Model):
    tank = models.ForeignKey(Tank)    
    sensor = models.ForeignKey(Sensor)
    time_offset = models.FloatField()
    value_offset = models.FloatField()
    start_time = models.DateTimeField()
    end_time = models.DateTimeField()

class TankLuxParams(models.Model):
    tank = models.ForeignKey(Tank)    
    sensor = models.ForeignKey(Sensor)
    time_offset = models.FloatField()
    value_offset = models.FloatField()
    start_time = models.DateTimeField()
    end_time = models.DateTimeField()
