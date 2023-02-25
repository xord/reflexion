# -*- mode: ruby; coding: utf-8 -*-


%w[xot rucy beeps rays reflex]
  .map  {|s| File.expand_path "#{s}/lib", __dir__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}


require 'xot/rake/util'

include Xot::Rake


EXTENSIONS = %i[xot rucy beeps rays reflex]
TASKS      = %i[vendor erb lib ext test gem install uninstall upload clean clobber]

TARGETS = []


def targets ()
  TARGETS.empty? ? EXTENSIONS : TARGETS
end

def sh_each_targets (cmd)
  targets.each do |t|
    cd_sh File.expand_path(t.to_s, __dir__), cmd
  end
end


task :default

task :run do
  raise unless name = env(:sample)
  sh %{ ruby reflex/samples/#{name}.rb }
end

TASKS.each do |name|
  task name => 'hooks:all' do
    sh_each_targets "rake #{name}"
  end
end

task :all do
  TARGETS.concat EXTENSIONS
end

task :debug do
  debug true
end

EXTENSIONS.each do |ext|
  task ext do
    TARGETS << ext
  end
end


namespace :subtree do
  github = 'git@github.com:xord'

  task :push do
    targets.each do |t|
      sh %( git subtree push --prefix=#{t} #{github}/#{t} master )
    end
  end

  task :pull do
    targets.each do |t|
      sh %( git subtree pull --prefix=#{t} #{github}/#{t} master )
    end
  end
end


namespace :hooks do
  hooks = Dir.glob('.githooks/*')
    .map {|path| [path, ".git/hooks/#{File.basename path}"]}.to_h

  hooks.each do |from, to|
    file to => from do
      sh %( cp #{from} #{to} )
    end
  end

  task :all => hooks.values
end
