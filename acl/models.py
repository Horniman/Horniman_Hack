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
    access = models.IntegerField()
    status = models.IntegerField()
    data = models.CharField(max_length=255)

class UserRoles(models.Model):
    user = models.ForeignKey(User)
    rid = models.IntegerField()

class Observations(models.Model):
    user = models.ForeignKey(User)
    subject = models.CharField(max_length=255)
    text = models.TextField()
    time = models.DateTimeField()
    reminder = models.DateTimeField()
        
class ObservationImages(models.Model):
    observation = models.ForeignKey(Observations)
    subject = models.CharField(max_length=255)
    text = models.TextField()
    file = models.CharField(max_length=255)
