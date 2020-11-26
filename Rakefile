# -*- mode: ruby; coding: utf-8 -*-


%w[xot rucy beeps rays reflex]
  .map  {|s| File.expand_path "#{s}/lib", __dir__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}


require 'xot/rake/util'

include Xot::Rake


MODULES = %i[xot rucy beeps rays reflex]
TASKS   = %i[vendor erb lib ext test gem install uninstall upload clean clobber]

TARGETS = []


def cd_sh (dir, cmd)
  Dir.chdir dir do
    $stderr.puts "(in #{Dir.pwd})"
    sh cmd
  end
end

def filter_file (path, &block)
  File.write path, block.call(File.read path)
end

def modified_files (dir: '.', hash: '@')
  `git diff --name-only #{hash} -- #{dir}`.lines.map &:chomp
end

def version (dir = '.')
  File.readlines("#{dir}/VERSION").first.chomp
end

def module_versions ()
  MODULES.each_with_object({}) do |mod, hash|
    hash[mod] = /(\d+)\.(\d+)\.(\d+)/.match(version mod)[1..3].map &:to_i
  end
end

def targets ()
  TARGETS.empty? ? MODULES : TARGETS
end

def sh_each_targets (cmd)
  targets.each do |target|
    cd_sh File.expand_path(target.to_s, __dir__), cmd
  end
end


task :default

task :run do
  raise unless name = env(:sample)
  sh %{ ruby reflex/samples/#{name}.rb }
end

TASKS.each do |name|
  task name do
    sh_each_targets "rake #{name}"
  end
end

task :all do
  TARGETS.concat MODULES
end

task :debug do
  debug true
end

MODULES.each do |mod|
  task mod do
    TARGETS << mod
  end
end


namespace :version do

  namespace :bump do

    def bump_version (ver, index)
      nums         = ver.split('.').map &:to_i
      nums        << 0 until nums.size > index
      nums[index] += 1
      nums.map!.with_index {|num, i| i > index ? 0 : num}
      nums.pop while nums.last == 0 && nums.size >= 3
      nums.join '.'
    end

    def update_version (index)
      oldver = version
      newver = bump_version oldver, index
      File.write 'VERSION', newver
      newver
    end

    def modified_targets ()
      targets.select {|t|
        modified_files(dir: t, hash: "v#{version t}").size > 0
      }
    end

    def update_module_versions ()
      modified_targets.each do |target|
        sh %( cp VERSION #{target}/ )
      end
    end

    def update_dependencies ()
      vers = module_versions
      targets.each do |target|
        ver = vers[target][0..2].join '.'
        re  = /add_runtime_dependency\s*['"]#{target}['"]\s*,\s*['"]~>\s*[\d\.]+['"]\s*$/
        Dir.glob('*/*.gemspec').each do |path|
          filter_file path do |gemspec|
            gemspec.sub(re) {|s| s.sub /[\d\.]+/, ver}
          end
        end
      end
    end

    def update_and_tag_version (index)
      raise 'modified files exist' unless modified_files.empty?

      raise 'no modified modules' if modified_targets.empty?

      message = ENV['message']
      raise 'no message' unless message

      newver = update_version index
      raise 'version is not updated' unless modified_files == ['VERSION']

      update_module_versions
      update_dependencies

      sh %( git add -u )
      sh %( git commit -m "#{message}" )
      sh %( git tag -a -m "#{message}" v#{newver} )
    end

    task :major do
      update_and_tag_version 0
    end

    task :minor do
      update_and_tag_version 1
    end

    task :patch do
      update_and_tag_version 2
    end

    task :build do
      update_and_tag_version 3
    end

  end# bump

end# version


namespace :subtree do

  github = 'git@github.com:xord'

  task :push do
    targets.each do |target|
      sh %( git subtree push --prefix=#{target} #{github}/#{target} master )
    end
  end

  task :pull do
    target.each do |target|
      sh %( git subtree pull --prefix=#{target} #{github}/#{target} master )
    end
  end

end# subtree
