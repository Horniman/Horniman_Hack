from django.db import models
from db import models as db_models

class LogTempLive(models.Model):
    sensor = models.ForeignKey(db_models.Sensor)
    processed = models.BooleanField()
    data = models.CharField(max_length=50)
    time = models.DateTimeField()

class LogTempBackup(models.Model):
    sensor = models.ForeignKey(db_models.Sensor)
    byyy = models.CharField(max_length=4)
    bm =models.CharField(max_length=2)
    bd =models.CharField(max_length=2)
    bh =models.CharField(max_length=2)
    eyyy = models.CharField(max_length=4)
    em =models.CharField(max_length=2)
    ed=models.CharField(max_length=2)
    eh=models.CharField(max_length=2)
    sst=models.FloatField()
    sstanom=models.FloatField()
    hotspot=models.FloatField()
    dhw=models.FloatField()
    lat=models.FloatField()
    long=models.FloatField()    
