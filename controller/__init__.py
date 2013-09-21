"""
"""
from __future__ import division, absolute_import, print_function, unicode_literals

from . import models
from db import models as db_models
from time import time

def proc_controller():
	now = time()
	for tank in models.tank.objects.all():
		profile = getProfile(tank)
		curtemp = getcurTemp(tank)
		curlux = getcurLux(tank)
		controlTemp(tank,profile.temp, curtemp)
		controlLux(tank,profile.lux, curlux)

def getProfile(tank):
	"""Find the most recent profile entry for this tank"""
	profile = models.Profile.objects.filter(tank_id = tank.id).filter(time_lte now).last()
	return profile

def getcurTemp(tank):
	"""Find the latest temperature reading for this tank"""
	#TODO
	return 0

def getcurLux(tank):
	"""Find the latest light reading for this tank"""
	#TODO
	return 0

def controlTemp(tank,target,actual):
	#TODO some sort of hysteresis or a PID control
	if target > actual:
		heaterOn(Tank)
	elif
		heaterOff(Tank)

def heaterOn(tank):
	#TODO

def heaterOff(tank):
	#TODO

def controlLux(tank,target,actual):
	#TODO some sort of hysteresis or a PID control
	if target > actual:
		lightOn(Tank)
	elif
		lightOff(Tank)

def lightOn(tank):
	#TODO

def lightOff(tank):
	#TODO
