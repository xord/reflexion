# -*- coding: utf-8 -*-


require 'reflex/ext'


module Reflex


  class ContactEvent

    def type ()
      TYPE2SYM[get_type] || :none
    end

    def begin? ()
      get_type == TYPE_BEGIN
    end

    def end? ()
      get_type == TYPE_END
    end

    def inspect ()
      "#<Reflex::ContactEvent type:#{type} view:#{view}>"
    end

    private

      TYPE2SYM = {
        TYPE_BEGIN => :begin,
        TYPE_END   => :end,
      }

  end# ContactEvent


end# Reflex
