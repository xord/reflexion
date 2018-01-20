# -*- mode: ruby; coding: utf-8 -*-


require 'rbconfig'


task :ext => 'ext:build'

%w[clean].each do |t|
  task t.intern => "ext:#{t}"
end


namespace :ext do


  rbconf = RbConfig::CONFIG

  mod      = MODULE
  mods     = env :MODULES,  [], :array
  modname  = env :MODNAME,  mod.name.downcase
  dlname   = env :DLNAME,   'native'
  dlext    = env :DLEXT,    rbconf['DLEXT'] || 'so'
  extdir   = env :EXTDIR,   "ext/#{modname}"
  libdir   = env :LIBDIR,   "lib/#{modname}"
  ruby     = env :RUBY,     'ruby'
  make     = env :MAKE,     'make'
  cxx      = env :CXX,      rbconf['CXX'] || 'g++'
  cppflags = env :CPPFLAGS, rbconf['CPPFLAGS']
  cxxflags = env :CXXFLAGS, rbconf['CXXFLAGS']
  defs     = env :DEFS,     [], :array
  incdirs  = env(:INCDIRS,  [], :array) + mods.reverse.map {|m| m.include_dir}

  cppflags = cppflags cppflags, defs, incdirs
  cxxflags = cflags   cxxflags

  outname = "#{dlname}.#{dlext}"
  extout  = File.join extdir, outname
  libout  = File.join libdir, outname

  srcs = FileList["#{extdir}/**/*.cpp"]
  libs = mods.map {|mod| mod.name.downcase}.map {|s| "../#{s}/lib/lib#{s}.a"}

  extconf  = File.join extdir, 'extconf.rb'
  makefile = File.join extdir, 'Makefile'
  depend   = File.join extdir, 'depend.mf'


  task :build => libout

  task :clean do
    sh %( cd #{extdir} && #{make} clean ) if File.exist? makefile
    sh %( rm -rf #{makefile} #{depend} #{libout} )
  end

  file libout => extout do
    sh %( cp #{extout} #{libout} )
  end

  file extout => [:lib, makefile] do
    sh %( cd #{extdir} && #{make} )
  end

  file makefile => [extconf, depend] + libs do
    sh %( cd #{extdir} && #{ruby} #{File.basename extconf} )
  end

  file depend => srcs do
    inc = incdirs.map {|s| " -I#{s}"}.join
    src = srcs.map {|cpp| File.basename cpp}.join ' '
    dep = File.basename depend
    sh %( cd #{extdir} && #{cxx} -M #{cppflags} #{inc} #{src} > #{dep} )
  end


end# :ext
