# -*- mode: ruby -*-


Pod::Spec.new do |s|
  s.name        = "Reflexion"
  s.version     = File.readlines(File.expand_path 'VERSION', __dir__)[0].chomp
  s.summary     = "Processing like framework for Ruby language."
  s.description = "Processing like framework for Ruby language embedded to OSX/iOS App."
  s.license     = "MIT"
  s.source      = {:git => "https://github.com/xord/reflexion.git"}
  s.author      = {"xordog" => "xordog@gmail.com"}
  s.homepage    = "https://github.com/xord/reflexion"

  s.osx.deployment_target = "10.10"
  s.ios.deployment_target = "10.0"

  root    = "${PODS_ROOT}/#{s.name}"
  mods    = %w[xot rucy beeps rays reflex]
  incdirs = mods.map {|m| "#{root}/#{m}/include"}.concat %W[
    #{root}/src
    #{root}/beeps/vendor/stk/include
    #{root}/rays/vendor/glm
    #{root}/rays/vendor/clipper/cpp
    #{root}/rays/vendor/poly2tri/poly2tri
    #{root}/rays/vendor/splines-lib
    #{root}/reflex/vendor/Box2D/Box2D
    ${PODS_ROOT}/CRuby/CRuby/include
  ]

  s.prepare_command    = "rake vendor erb"
  s.preserve_paths     = mods + %w[src]
  s.requires_arc       = false
  s.osx.compiler_flags = "-DOSX"
  s.ios.compiler_flags = "-DIOS"
  s.library            = %w[c++]
  s.xcconfig           = {
    "CLANG_CXX_LANGUAGE_STANDARD" => 'c++20',
    "HEADER_SEARCH_PATHS"         => incdirs.join(' ')
  }
  s.resource_bundles   = mods.each_with_object({}) do |mod, hash|
    hash[mod.capitalize] = %W[#{mod}/lib VERSION]
  end

  #s.dependency = 'CRuby', git: 'https://github.com/xord/cruby'

  s.subspec "Xot" do |spec|
    mod = spec.name.split('/').last.downcase

    spec.source_files = "#{mod}/src/*.cpp"
  end

  s.subspec "Rucy" do |spec|
    mod = spec.name.split('/').last.downcase

    spec.source_files = "#{mod}/src/*.cpp"

    spec.subspec "Ext" do |ext|
      ext.source_files = "#{mod}/ext/#{mod}/*.cpp"
    end
  end

  s.subspec "Beeps" do |spec|
    mod = spec.name.split('/').last.downcase

    spec.source_files = "#{mod}/src/*.cpp"
    spec.frameworks   = %w[OpenAL]

    spec.subspec "STK" do |sub|
      sub.source_files  = "#{mod}/vendor/stk/src/*.cpp"
      sub.exclude_files = %W[Tcp Udp Socket Thread Mutex InetWv Rt].map {|s|
        "#{mod}/vendor/stk/src/#{s}*.cpp"
      }
    end

    spec.subspec "Ext" do |ext|
      ext.source_files = "#{mod}/ext/#{mod}/*.cpp"
    end
  end

  s.subspec "Rays" do |spec|
    mod = spec.name.split('/').last.downcase

    spec    .source_files = "#{mod}/src/*.cpp"
    spec.osx.source_files = "#{mod}/src/osx/*.{cpp,mm}"
    spec.ios.source_files = "#{mod}/src/ios/*.{cpp,mm}"
    spec.ios.frameworks   = %w[GLKit MobileCoreServices AVFoundation]# ImageIO

    spec.subspec "Clipper" do |sub|
      sub.source_files = "#{mod}/vendor/clipper/cpp/*.cpp"
    end

    spec.subspec "Poly2Tri" do |sub|
      sub.source_files = "#{mod}/vendor/poly2tri/poly2tri/**/*.cc"
    end

    spec.subspec "SplineLib" do |sub|
      sub.source_files = "#{mod}/vendor/splines-lib/Splines.cpp"
    end

    spec.subspec "Ext" do |ext|
      ext.source_files = "#{mod}/ext/#{mod}/*.cpp"
    end
  end

  s.subspec "Reflex" do |spec|
    mod = spec.name.split('/').last.downcase

    spec    .source_files = "#{mod}/src/*.cpp"
    spec.osx.source_files = "#{mod}/src/osx/*.{cpp,mm}"
    spec.ios.source_files = "#{mod}/src/ios/*.{cpp,mm}"
    spec.ios.frameworks   = %w[CoreMotion]

    spec.subspec "Box2D" do |sub|
      sub.source_files = "#{mod}/vendor/Box2D/Box2D/Box2D/**/*.cpp"
    end

    spec.subspec "Ext" do |ext|
      ext.source_files = "#{mod}/ext/#{mod}/*.cpp"
    end
  end

  s.subspec "Reflexion" do |spec|
    spec.source_files = "src/*.m"
  end
end
