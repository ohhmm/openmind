set print pretty on
set print object on
set print static-members on
set print vtbl on
set print demangle on
set demangle-style gnu-v3
set print sevenbit-strings off

break Sum::optimize
commands
  silent
  if GetView() == View::SupersetOfRoots
    print "Found SupersetOfRoots view"
    print "Members:"
    print members
    bt
  end
  continue
end

break PrincipalSurd::optimize
commands
  silent
  print "PrincipalSurd::optimize called"
  print "Radicand:"
  print _1
  print "Index:"
  print _2
  continue
end

break Sum.cpp:2032
commands
  silent
  print "Testing negative result for complex roots"
  print result
  print "Absolute value:"
  print absValue
  print "Square root of abs:"
  print sqrtAbs
  print "Imaginary solution:"
  print imagSolution
  continue
end

break Radicals.cpp:97
commands
  silent
  print "Starting RadicalSolving_test"
  print _1
  continue
end

break Radicals.cpp:105
commands
  silent
  print "Solutions found:"
  print solutions
  continue
end
