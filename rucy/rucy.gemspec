# -*- mode: ruby; coding: utf-8 -*-


File.expand_path('../lib', __FILE__)
  .tap {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'rucy/module'


Gem::Specification.new do |s|
  glob = -> *patterns do
    patterns.map {|pat| Dir.glob(pat).to_a}.flatten
  end

  mod   = Rucy::Module
  name  = mod.name.downcase
  rdocs = glob.call *%w[README .doc/ext/**/*.cpp]

  s.name        = name
  s.summary     = 'A Ruby C++ Extension Helper Library.'
  s.description = 'This library helps you to develop Ruby Extension by C++.'
  s.version     = mod.version

  s.authors  = %w[snori]
  s.email    = 'snori@xord.org'
  s.homepage = "https://github.com/xord/rucy"

  s.platform              = Gem::Platform::RUBY
  s.required_ruby_version = '>=1.9.0'

  s.add_runtime_dependency 'rake'
  s.add_runtime_dependency 'xot'
  s.add_development_dependency 'gemcutter'

  s.files            = `git ls-files`.split $/
  s.executables      = s.files.grep(%r{^bin/}) {|f| File.basename f}
  s.test_files       = s.files.grep %r{^(test|spec|features)/}
  s.extra_rdoc_files = rdocs.to_a
  s.has_rdoc         = true

  s.extensions << 'Rakefile'
end
