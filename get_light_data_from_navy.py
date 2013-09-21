import requests
import mechanize
import re
import datetime
from sys import argv


br=mechanize.Browser()
br.set_handle_robots(False)

#year, month, day, placename, longdegree, longminute, latdegree, latminute, tzoffset = "2013 9 19 london 1 2 52 3 1".split(' ')
print "USAGE: blah.py yyyy m dd placename LAT lat LONG long TZOFFSET"
x, year, month, day, placename, longdegree, longminute, latdegree, latminute, tzoffset = argv

#FFX=2
#ID=AA
#xxy=2013
#xxm=9
#xxd=21
#place=%28no+name+given%29
#xx0=-1
#xx1=lde
#xx2=lm
#yy0=1
#yy1=od
#yy2=om
#zz1=tz
#zz0=-1
#ZZZ=END

br.open("http://aa.usno.navy.mil/data/docs/RS_OneDay.php")
br.select_form(nr=2)


#br['FFX'] = '2'
#br['ID'] = 'AA'
br['xxy'] = year
br['xxm'] = [month]
br['xxd'] = day
br['place'] = placename
#br['xx0'] = -1
br['xx1'] = longdegree
br['xx2'] = longminute
#br['yy0'] = 1
br['yy1'] = latdegree
br['yy2'] = latminute
br['zz1'] = tzoffset
#br['zz0'] = -1
#br['ZZZ'] = 'END'

r=br.submit()

page= r.read()

twilight_start_re = re.compile("Begin civil twilight      (.*)")
sunrise_time_re = re.compile("Sunrise                   (.*)")
sun_transit_re = re.compile("Sun transit               (.*)")
sunset_time_re = re.compile("Sunset                    (.*)")
twilight_end_re = re.compile("End civil twilight        (.*)")

moonrise_re = re.compile("Moonrise.*([0-1][0-9]:[0-5][0-9]) [^on]")
moon_transit_re = re.compile("Moon transit              (.*)")
moonset_re = re.compile("Moonset.*([0-1][0-9]:[0-5][0-9]) [^on]")

twilight_start= twilight_start_re.findall(page)[0].strip()
sunrise_time = sunrise_time_re.findall(page)[0].strip()
sun_transit = sun_transit_re.findall(page)[0].strip()
sunset_time = sunset_time_re.findall(page)[0].strip()
twilight_end = twilight_end_re.findall(page)[0].strip()

moonrise = moonrise_re.findall(page)[0].strip()
moon_transit = moon_transit_re.findall(page)[0].strip()
moonset = moonset_re.findall(page)[0].strip()

print datetime.datetime(int(year), int(month), int(day))
print twilight_start, sunrise_time, sun_transit, sunset_time, twilight_end

print moonrise, moon_transit, moonset

