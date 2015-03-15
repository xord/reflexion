# -*- coding: utf-8 -*-


require 'xot/rake'
require 'xot/block_util'


module Xot


  class ExtConf

    include Xot::Rake

    attr_reader :modules, :defs, :include_dirs, :lib_dirs, :headers, :libs, :local_libs, :frameworks

    def initialize (*modules, &block)
      @modules = modules.map {|m| m.const_get :Module}
      @defs, @include_dirs, @lib_dirs, @headers, @libs, @local_libs, @frameworks =
        ([[]] * 7).map &:dup
      Xot::BlockUtil.instance_eval_or_block_call self, &block if block
    end

    def debug ()
      env :DEBUG, false
    end

    def setup ()
      yield if block_given?

      modules.each do |m|
        name = m.name.downcase
        headers << "#{name}.h"
        libs << name
      end

      local_libs << (clang? ? 'c++' : 'stdc++')

      $CPPFLAGS = cppflags $CPPFLAGS, defs, include_dirs
      $CFLAGS   = cflags   $CFLAGS + ' -x c++'
      $LDFLAGS  = ldflags  $LDFLAGS, lib_dirs, frameworks
      $LOCAL_LIBS << local_libs.map {|s| " -l#{s}"}.join
    end

    def create_makefile (*args)
      modules.each do |m|
        dir_config m.name.downcase, m.include_dir, m.lib_dir
      end

      exit 1 unless headers.all? {|s| have_header s}
      exit 1 unless libs.all?    {|s| have_library s, 't'}

      super
    end

  end# ExtConf


end# Xot
