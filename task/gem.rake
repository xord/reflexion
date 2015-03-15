# -*- mode: ruby; coding: utf-8 -*-


task :gem => 'gem:build'

%w[clean install uninstall upload].each do |t|
  task t.intern => "gem:#{t}"
end

empty_task :lib, :ext, :rdoc


namespace :gem do


  mod  = MODULE
  name = env :NAME, MODULE.name.downcase
  gem  = env :GEM,  'gem'

  gemspec = "#{name}.gemspec"
  gemname = env :GEMNAME,    name
  gemver  = env :GEMVERSION, mod.version
  gemfile = "#{gemname}-#{gemver}.gem"


  task :build => gemfile

  task :clean do
    sh %( rm -f #{gemfile} )
  end

  task :install => gemfile do
    sh %( #{gem} install #{gemfile} )
  end

  task :uninstall do
    sh %( #{gem} uninstall -x --version #{gemver} #{gemname} )
  end

  task :test => [:install, :uninstall]

  task :upload => :install do
    sh %( #{gem} push #{gemfile} )
  end

  file gemfile => [:ext, :rdoc] do
    sh %( #{gem} build #{gemspec} )
  end


end# :gem
