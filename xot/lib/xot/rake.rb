# -*- coding: utf-8 -*-


require 'erb'
require 'rbconfig'
require 'pp'


module Xot


  module Rake


    def empty_task (*names)
      names.each do |name|
        task name => '.'
      end
    end

    def glob (*patterns)
      paths = []
      patterns.each do |pattern|
        paths.concat Dir.glob(pattern)
      end
      paths
    end

    def erb (str)
      ERB.new(str, nil, '%').result binding
    end

    def compile (path, out)
      open(path) do |input|
        open(out, 'w') do |output|
          output.write erb(input.read)
        end
      end
    #rescue
    end

    def params (max, sep = '', &block)
      raise 'block not given.' unless block
      return '' if max == 0
      (1..max).map(&block).join(sep)
    end

    def convertions (paths, convs)
      raise 'empty conversion.' if convs.empty?
      paths = paths.map do |path|
        convpath = path
        convs.each do |from, to|
          convpath = convpath.sub(/#{from.gsub('.', '\.')}$/, to)
        end
        [path, convpath]
      end
      Hash[*paths.flatten]
    end

    def env (name, defval = nil, to = nil)
      case val = (ENV[name.to_s] || Object.const_get(name) rescue defval)
      when /^\d+$/        then val.to_i
      when 'true',  true  then true
      when 'false', false then false
      when nil            then nil
      when Numeric        then val
      else
        val = val.split /\s+/ if to == :array && val.respond_to?(:split)
        val.dup
      end
    end

    def ruby_incdirs ()
      root = RbConfig::CONFIG['rubyhdrdir']
      [root, RbConfig::CONFIG['rubyarchhdrdir'] || "#{root}/#{RUBY_PLATFORM}"]
    end

    def cppflags (flags = '', defs = [], incdirs = [])
      s = flags.dup
      s << cppflags_defs(defs)      .map {|s| " -D#{s}"}.join
      s << cppflags_incdirs(incdirs).map {|s| " -I#{s}"}.join
      s
    end

    def cppflags_defs (defs = [])
      a = defs
      a << $~[0].upcase if RUBY_PLATFORM =~ /mswin|ming|cygwin|darwin/i
      a << (debug? ? '_DEBUG' : 'NDEBUG')
      a << 'WIN32' if win32?
      a << 'OSX'   if osx?
      a << 'IOS'   if ios?
      a
    end

    def cppflags_incdirs (dirs = [])
      a = dirs.dup
      a += ruby_incdirs
      a
    end

    def cflags (flags = '')
      s = flags.dup
      s << ' -Wno-unknown-pragmas'
      s << ' ' << RbConfig::CONFIG['debugflags'] if debug?
      s << ' -std=c++11 -stdlib=libc++ -mmacosx-version-min=10.7' if clang?
      s.gsub! /-O\d?/, '-O0' if debug?
      s
    end

    def ldflags (flags = '', libdirs = [], frameworks = [])
      s = flags.dup
      s << libdirs.map    {|s| " -L#{s}"}.join
      s << frameworks.map {|s| " -framework #{s}"}.join
      s
    end

    def header (mod)
      puts "-- #{mod.to_s.capitalize} " + '-' * 50
    end

    def debug? ()
      env :DEBUG, false
    end

    def win32? ()
      RUBY_PLATFORM =~ /mswin|ming|cygwin/
    end

    def mswin? ()
      RUBY_PLATFORM =~ /mswin/
    end

    def ming? ()
      RUBY_PLATFORM =~ /ming/
    end

    def cygwin? ()
      RUBY_PLATFORM =~ /cygwin/
    end

    def osx? ()
      RUBY_PLATFORM =~ /darwin/
    end

    def ios? ()
      false
    end

    def clang? ()
      RbConfig::CONFIG['CXX'] =~ /clang/
    end


  end# Rake


end# Xot


if Kernel.const_defined? :Rake
  module Rake
    class Application
      alias invoke_task_org invoke_task

      def invoke_task (*args, &block)
        invoke_task_org *args, &block
      rescue RuntimeError => e
        if e.message =~ /don't\s+know\s+.*\s+task\s+/i
          escalate_tasks
        else
          raise
        end
      end

      def escalate_tasks ()
        Dir.chdir '..' do
          cmd = "rake #{ARGV.join ' '}"
          $stderr.puts "(in #{Dir.pwd}) #{cmd}"
          system cmd
        end
        exit 0
      end
    end
  end
end
