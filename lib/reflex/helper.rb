# -*- coding: utf-8 -*-


require 'xot/hookable'
require 'reflex/point'
require 'reflex/bounds'


module Reflex


  module Hookable

    include Xot::Hookable

    def hook (name, &block)
      super "on_#{name}".intern, &block
    end

  end# Hookable


  module HasFrame

    def frame= (*args)
      set_frame args.flatten
      get_frame
    end

    def frame (*args)
      send :frame=, *args unless args.empty?
      get_frame
    end

    def move_to (*args)
      self.frame = get_frame.move_to *args
      self
    end

    def move_by (*args)
      self.frame = get_frame.move_by *args
      self
    end

    def resize_to (*args)
      self.frame = get_frame.resize_to *args
      self
    end

    def resize_by (*args)
      self.frame = get_frame.resize_by *args
      self
    end

    def inset_by (*args)
      self.frame = get_frame.inset_by *args
      self
    end

    %w[
      x y z w h d width height depth
      left top back right bottom front
      left_top right_top left_bottom right_bottom
      lt rt lb rb
      position pos size center
    ].each do |name|
      name_assign = "#{name}=".intern
      name        = name.intern

      define_method name do |*args|
        send name_assign, *args unless args.empty?
        get_frame.send name
      end

      define_method name_assign do |*args|
        b   = get_frame
        ret = b.send name_assign, *args
        set_frame b
        ret
      end
    end

  end# HasFrame


  module HasTags

    def tag= (*tags)
      clear_tags
      tag *tags.flatten
    end

    alias tags= tag=

    def tag (*tags)
      tags.each {|tag| add_tag tag}
    end

    def untag (*tags)
      tags.each {|tag| remove_tag tag}
    end

    def tags ()
      to_enum :each_tag
    end

    def clear_tags ()
      untag *tags.to_a
    end

  end# HasTags


end# Reflex
