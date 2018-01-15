# -*- mode: ruby; coding: utf-8 -*-


require 'rbconfig'
require 'rake/loaders/makefile'


task :lib => 'lib:build'

%w[erb clean].each do |t|
  task t.intern => "lib:#{t}"
end


namespace :lib do


  rbconf = RbConfig::CONFIG

  mod      = MODULE
  mods     = env :MODULES,  [], :array
  modname  = env :MODNAME,  MODULE.name.downcase
  incdir   = env :INCDIR,   'include'
  srcdir   = env :SRCDIR,   'src'
  libdir   = env :LIBDIR,   'lib'
  cxx      = env :CXX,      rbconf['CXX'] || 'g++'
  ar       = env :AR,       rbconf['AR']  || 'ar'
  cppflags = env :CPPFLAGS, rbconf['CPPFLAGS']
  cxxflags = env :CXXFLAGS, rbconf['CXXFLAGS']
  arflags  = env :ARFLAGS,  'crs'
  defs     = env :DEFS,     [], :array
  incdirs  = env(:INCDIRS,  [], :array) + mods.reverse.map {|m| m.include_dir}

  cppflags = cppflags cppflags, defs, incdirs
  cxxflags = cflags cxxflags

  outname = "lib#{modname}.a"
  out     = File.join libdir, outname

  erbs  = glob *[incdir, srcdir].map {|s| "#{s}/**/*.erb"}
  erbs  = convertions erbs, {".erb" => ""}
  srcs  = glob("#{srcdir}/**/*.cpp")   | erbs.values.grep(/\.cpp$/)
  srcs += glob("#{srcdir}/**/*.mm") if osx? || ios?
  srcs  = srcs.reject {|s| s =~ %r(/win32/)} unless win32?
  srcs  = srcs.reject {|s| s =~ %r(/osx/)}   unless osx?
  srcs  = srcs.reject {|s| s =~ %r(/ios/)}   unless ios?

  depend = 'depend.mf'
  objs   = convertions srcs, {".cpp" => ".o", ".mm" => ".o"}
  tmps   = (objs.values | erbs.values) + [depend]


  task :build => out

  task :clean do
    sh %( rm -rf #{tmps.join " "} #{out} )
  end

  task :compile => objs.values

  task :erb => erbs.values

  file out => objs.values do
    unless objs.values.empty?
      sh %( rm -f #{out} )
      sh %( #{ar} #{arflags} #{out} #{objs.values.join " "} )
    end
  end

  file depend => erbs.values do
    sh %( #{cxx} -M #{cppflags} #{srcs.join ' '} > #{depend} )
    input = open(depend) {|f| f.read}
    open(depend, 'w') do |output|
      output << input.gsub(/\w+\.o/, srcdir + '/\0')
    end
  end

  import depend if File.exist? depend

  objs.each do |src, obj|
    file obj => [depend, src] + erbs.values do
      sh %( #{cxx} -c #{cppflags} #{cxxflags} -o #{obj} #{src} )
    end
  end

  erbs.each do |erb, out|
    file out => erb do
      print "#{erb}: compiling to #{out} ..."
      compile erb, out
      puts "ok"
    end
  end


end# :lib
