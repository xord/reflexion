# -*- coding: utf-8 -*-


require 'reflex/ext'


module Reflex


  class StyleLength

    def initialize (*args)
      return if args.empty?
      case args[0]
      when Numeric
        args << :px if args.size < 2
        setup_num *args
      when String, Symbol
        setup_str *args
      end
    end

    def inspect ()
      "#<#{self.class.name} #{to_s}>"
    end

  end# StyleLength


end# Reflex
