
from db.models import TankTempParams
from db.models import Temp
from db.models import Profile
from timedate import timedelta


def SetProfileTable(ForDay, ForTank):

   TempParamSets = TankTempParams.objects.get(tank=ForTank)

   TempParamSet = TempParamSets.objets.get(start_time__gte = ForDay, end_time__lte = ForDay)
   if len(TempParamSet) <> 1:
      return 1 # error number of TemParamaterSets 
   LuxParamSets = TankLuxParams.objects.get(tank=ForTank)
   
   LuxParamSet = LuxParamSets.objets.get(start_time__gte = ForDay, end_time__lte = ForDay)
   if len(TempParamSet) <> 1:
      return 1 # error number of LuxParamaterSets 
   
   TempTimeOffset=TempParamSet[0].time_offset_seconds
   LuxTimeOffset=LuxParamSet[0].time_offset_seconds
   
   for TimePeriod in range(0,23):
      StartTime = ForDay.replace(hour=TimePeriod) - timedelta(seconds=TempTimeOffset)
      EndTime = ForDay.replace(hour=TimePeriod,minute=59)

      TempReadings = Temp.objects.get(time__gte=StartTime, time__lte=EndTime)
      SumTemp=0
      NumReadings=0
      for Reading in TempReadings:
         SumTemp=+ Reading.temp
         NumReadings=+1
      AveTemp[TimePeriod] = SumTemp/NumReadings

      
      LuxReadings = Lux.objects.get(time__gte=StartTime, time__lte=EndTime)
      SumLux=0
      NumReadings=0
      for Reading in LuxReadings:
         SumLux=+ Reading.Lux
         NumReadings=+1
      AveLux[TimePeriod] = SumLux/NumReadings
      
      ProfileTargetEntry = Profile(tank=ForTank, time=ForDay.replace(hour=TimePeriod), temp=AveTemp, lux=AveLux)
      ProfileTargetEntry.save()


