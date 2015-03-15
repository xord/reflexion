# -*- mode: ruby; coding: utf-8 -*-


require 'rake/testtask'


task :test => 'test:all'

empty_task :ext


namespace :test do


  alones   = env :TESTS_ALONE,   []
  excludes = env :TESTS_EXCLUDE, []
  ruby     = env :RUBY,          'ruby'


  task :all => [:ext, :full, :alones]

  Rake::TestTask.new :full do |t|
    t.libs << 'lib'
    t.test_files = FileList['test/test_*.rb'] - alones - excludes
    t.verbose = true
  end

  task :alones do
    alones.each do |rb|
      next if excludes.include? rb
      sh %( #{ruby} #{rb} )
    end
  end

end# :test
