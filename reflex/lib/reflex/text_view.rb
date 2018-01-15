# -*- coding: utf-8 -*-


require 'reflex/image'
require 'reflex/bitmap'
require 'reflex/view'


module Reflex


  class TextView < View

    has_model

    attr_accessor :font

    def initialize (*args, &block)
      @outdated = false
      super
    end

    def content_size ()
      s = string
      return 0, 0 unless s && window
      font = @font || window.painter.font
      [font.width(s), font.height].map &:ceil
    end

    def on_draw (e)
      @image = create_text_image if @outdated
      return unless @image

      pa = e.painter
      pa.fill 1
      pa.image @image
    end

    def on_data_update (e)
      @outdated = true
      super
    end

    private

      def create_text_image ()
        str, size = string, content_size
        return nil unless str && size.all? {|n| n > 0}
        Image.new(*size, :ALPHA).paint {text str}
      end

  end# TextView


end# Reflex
