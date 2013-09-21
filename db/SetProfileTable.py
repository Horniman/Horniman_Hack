
from db.models import TankTempParams
from db.models import Temp
from db.models import Profile

ParamSets = TankTempParams.objects.all()

for ParamSet in ParamSets do
   StartTime = ParamSet.start_time
   EndTime = ParamSet.end_time
   TempReadings = Temp.objects.get(time__gte=StartTime time__lte=EndTime)
   
   for Reading in TempReadings do
   

