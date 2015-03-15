# -*- mode: ruby; coding: utf-8 -*-


task :app => 'app:build'

%w[clean run].each do |t|
  task t.intern => "app:#{t}"
end


namespace :app do


  mod     = MODULE
  name    = env :NAME,   MODULE.name.downcase
  bindir  = env :BINDIR, 'bin'
  ruby    = env :RUBY,   'ruby'

  bin       = "#{bindir}/#{name}"
  appname   = name.capitalize
  appdir    = "#{appname}.app"
  appbindir = "#{appdir}/Contents/MacOS"
  out       = "#{appbindir}/#{name}"

  tmps = [appdir]


  task :build => out

  task :clean do
    sh %( rm -rf #{tmps.join ' '} )
  end

  task :run => :app do
    sh %( #{ruby} #{bin} )
  end

  file out => [bin, appbindir] do
    sh %( cp #{bin} #{appbindir} )
  end

  directory appbindir


end# :app
