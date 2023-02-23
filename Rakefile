# -*- mode: ruby; coding: utf-8 -*-


%w[xot rucy beeps rays reflex]
  .map  {|s| File.expand_path "#{s}/lib", __dir__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}


require 'xot/rake/util'

include Xot::Rake


EXTENSIONS = %i[xot rucy beeps rays reflex]
TASKS      = %i[vendor erb lib ext test gem install uninstall upload clean clobber]

TARGETS = []


def extension_versions ()
  EXTENSIONS.each_with_object({}) do |ext, hash|
    hash[ext] = get_version(ext).split('.').map(&:to_i)
  end
end

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


namespace :version do

  namespace :bump do

    def update_extension_versions ()
      targets.each do |t|
        sh %( cp #{VERSION_NAME} #{t}/ )
      end
    end

    def update_dependencies ()
      update_extension_versions

      vers = extension_versions
      targets.each do |t|
        ver = vers[t][0..2].join '.'
        re  = /add_runtime_dependency\s*['"]#{t}['"]\s*,\s*['"]~>\s*[\d\.]+['"]\s*$/
        Dir.glob('*/*.gemspec').each do |path|
          filter_file path do |gemspec|
            gemspec.sub(re) {|s| s.sub /[\d\.]+/, ver}
          end
        end
      end
    end

    task :major do
      update_and_tag_version 0 do
        update_dependencies
      end
    end

    task :minor do
      update_and_tag_version 1 do
        update_dependencies
      end
    end

    task :patch do
      update_and_tag_version 2 do
        update_dependencies
      end
    end

    task :build do
      update_and_tag_version 3 do
        update_dependencies
      end
    end

  end# bump

end# version


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

end# subtree
