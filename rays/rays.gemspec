# -*- mode: ruby; coding: utf-8 -*-


File.expand_path('lib', __dir__)
  .tap {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'rays/extension'


Gem::Specification.new do |s|
  glob = -> *patterns do
    patterns.map {|pat| Dir.glob(pat).to_a}.flatten
  end

  ext   = Rays::Extension
  name  = ext.name.downcase
  rdocs = glob.call *%w[README .doc/ext/**/*.cpp]

  s.name        = name
  s.summary     = 'A Drawing Engine using OpenGL.'
  s.description = 'This library helps you to develop graphics application with OpenGL.'
  s.version     = ext.version

  s.authors  = %w[xordog]
  s.email    = 'xordog@gmail.com'
  s.homepage = "https://github.com/xord/rays"

  s.platform              = Gem::Platform::RUBY
  s.required_ruby_version = '>= 2.7.0'

  s.add_runtime_dependency 'xot',  '~> 0.1.31'
  s.add_runtime_dependency 'rucy', '~> 0.1.30'

  s.files            = `git ls-files`.split $/
  s.executables      = s.files.grep(%r{^bin/}) {|f| File.basename f}
  s.test_files       = s.files.grep %r{^(test|spec|features)/}
  s.extra_rdoc_files = rdocs.to_a
  s.has_rdoc         = true

  s.extensions << 'Rakefile'
end
