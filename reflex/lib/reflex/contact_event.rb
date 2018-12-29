# -*- coding: utf-8 -*-


require 'xot/const_symbol_accessor'
require 'reflex/ext'


module Reflex


  class ContactEvent < Event

    alias get_type type

    const_symbol_reader :type, {
      none:  TYPE_NONE,
      begin: TYPE_BEGIN,
      end:   TYPE_END
    }

    def begin? ()
      get_type == TYPE_BEGIN
    end

    def end? ()
      get_type == TYPE_END
    end

    def inspect ()
      "#<Reflex::ContactEvent type:#{type} shape:#{shape} view:#{view}>"
    end

  end# ContactEvent


end# Reflex
