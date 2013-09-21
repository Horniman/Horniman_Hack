import requests
import mechanize

br=mechanize.Browser()
br.set_handle_robots(False)

year, month, day, placename, longdegree, longminute, latdegree, latminute, tzoffset = "2013 9 19 london 1 2 52 3 1".split(' ')

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

print br.form

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

#r=requests.post('http://aa.usno.navy.mil/cgi-bin/aa_pap.pl', f)
print dir(r)
print r.read()
#print r.request
#print r.text
