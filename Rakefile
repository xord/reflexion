# -*- mode: ruby; coding: utf-8 -*-


%w[xot rucy rays reflex]
  .map  {|s| File.expand_path "../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}


MODULES = %w[xot rucy rays reflex].map &:intern
TASKS   = %w[erb lib ext test clean gem install uninstall upload].map &:intern

TARGETS = []


def env (key, defval = nil)
  val = ENV[key.to_s]
  val == nil ? defval : val
end

def cd_sh (dir, cmd)
  Dir.chdir dir do
    $stderr.puts "(in #{Dir.pwd})"
    sh cmd
  end
end

def sh_each_targets (cmd)
  TARGETS.each do |target|
    cd_sh File.expand_path("../#{target}", __FILE__), cmd
  end
end


task :default

task :run do
  raise unless name = env(:sample)
  sh %( ruby reflex/samples/#{name}.rb ) 
end

TASKS.each do |name|
  task name do
    sh_each_targets %( rake #{name} )
  end
end

task :all do
  TARGETS.concat MODULES
end

MODULES.each do |mod|
  task mod do
    TARGETS << mod
  end
end
