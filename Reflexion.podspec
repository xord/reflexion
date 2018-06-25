# -*- mode: ruby -*-

Pod::Spec.new do |s|
  s.name         = "Reflexion"
  s.version      = "0.1.0"
  s.summary      = "Processing like framework for Ruby language."
  s.description  = <<-END
                   Processing like framework for Ruby language embedded to OSX/iOS App.
                   END
  s.license      = "MIT"
  s.source       = {:git => "https://github.com/xord/reflexion"}
  s.author       = {"xordog" => "xordog@gmail.com"}
  s.homepage     = "https://github.com/xord"

  s.osx.deployment_target = "10.7"
  s.ios.deployment_target = "7.0"

  root     = "${PODS_ROOT}/#{s.name}"
  mods     = %w[xot rucy beeps rays reflex]
  inc_dirs = mods.map {|m| "#{root}/#{m}/include"}.concat %W[
    #{root}
    #{root}/beeps/src/stk/include
    #{root}/reflex/src/physics
  ]

  s.preserve_paths   = mods
  s.requires_arc     = false
  s.source_files     = mods.map {|m| "#{m}/src/*.cpp"}.concat %w[
    beeps/src/stk/**/*.cpp
    beeps/ext/**/*.cpp
    rays/ext/**/*.cpp
    reflex/src/physics/**/*.cpp
    reflex/ext/**/*.cpp
    *.m
  ]
  s.library          = %w[c++]
  s.resource_bundles = mods.inject({}) {|h, m| h[m.capitalize] = "#{m}/lib"; h}
  s.xcconfig         = {
    "HEADER_SEARCH_PATHS" => inc_dirs.join(' '),
    "OTHER_CFLAGS"        => "-I/usr/local/include"
  }

  s.osx.source_files  = mods.map {|m| "#{m}/src/osx/*.{cpp,mm}"}
  s.ios.source_files  = mods.map {|m| "#{m}/src/ios/*.{cpp,mm}"}
  s.osx.frameworks     = %w[OpenAL]
  s.ios.frameworks     = %w[OpenAL GLKit ImageIO MobileCoreServices CoreMotion]
  s.osx.compiler_flags = "-DOSX"
  s.ios.compiler_flags = "-DIOS"
  s.osx.xcconfig = {"HEADER_SEARCH_PATHS" => "${PODS_ROOT}/CRuby/build/CRuby_osx.framework/Headers"}
  s.ios.xcconfig = {"HEADER_SEARCH_PATHS" => "${PODS_ROOT}/CRuby/build/CRuby_ios.framework/Headers"}

  #s.dependency = 'CRuby', git: 'https://github.com/xord/cruby'

  s.prepare_command = "rake #{mods.join ' '} erb"
end
