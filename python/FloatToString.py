def FloatToString(inputValue):
   return ('%.10f' % inputValue).rstrip('0').rstrip('.')
def FloatToStringScientific(inputValue,etype='g'):
   s = FloatToString(inputValue)
   q = s.replace(".","").lstrip('0')
   nq = max(len(q)-1, 4)
   strcmd = ("%%%c%i%c" % ('.',nq,etype))
   return (strcmd % inputValue)
def FloatToStringMixedStyle(inputValue,etype='g'):
   if inputValue>=0.01:
      return FloatToString(inputValue)
   else:
      return FloatToStringScientific(inputValue,etype)
