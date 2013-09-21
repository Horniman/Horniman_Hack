"""
Generic tables
"""
from __future__ import division, absolute_import, print_function, unicode_literals

from django.db import models

class User(models.Model):
    """Seperate control zones"""
    name = models.CharField(max_length=255)
    password = models.CharField(max_length=128)
    mail = models.CharField(max_length=255)
    access = models.IntField()
    status = models.IntField()
    data = models.CharField(max_length=255)

class UserRoles(model.Model):
    user = models.ForiegnKey(User)
    rid = models.IntField()

class Observations(model.Model):
    user = models.ForiegnKey(User)
    subject = models.CharField(max_length=255)
    text = models.TextField()
    time = models.DateTimeField()
    reminder = models.DateTimeField()
        
class ObservationImages(model.Model):
    observation = models.ForiegnKey(Observations)
    subject = models.CharField(max_length=255)
    text = models.TextField()
    file = models.FileField()
