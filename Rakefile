# -*- mode: ruby; coding: utf-8 -*-


%w[xot rucy beeps rays reflex]
  .map  {|s| File.expand_path "../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}


require 'xot/rake'

include Xot::Rake


MODULES = %w[xot rucy beeps rays reflex].map &:intern
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

def each_target (&block)
  TARGETS.each do |target|
    block.call target
  end
end

def sh_each_targets (cmd)
  each_target do |target|
    cd_sh File.expand_path("../#{target}", __FILE__), cmd
  end
end


task :default

task :run do
  raise unless name = env(:sample)
  sh ruby "reflex/samples/#{name}.rb"
end

TASKS.each do |name|
  task name do
    sh_each_targets rake name
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


namespace :version do

  task :update do
    each_target do |target|
      sh %( cp VERSION #{target}/ )
    end
  end

end# version


namespace :subtree do

  github = 'git@github.com:xord'

  task :push do
    each_target do |target|
      sh %( git subtree push --prefix=#{target} #{github}/#{target} master )
    end
  end

  task :pull do
    each_target do |target|
      sh %( git subtree pull --prefix=#{target} --squash #{github}/#{target} master )
    end
  end

end# subtree
