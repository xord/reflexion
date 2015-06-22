# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'
include Reflex


module Reflex
  class View
    def view (frame = [0, 0, rand(10..200), rand(10..200)], color = [rand, rand, rand], &block)
      v = RectShape.new &block
      v.set frame: frame, fill: Color.color(*color)
      add v
      v
    end

    alias text view
    alias image view
    alias button view
  end
end

win = Window.new title: 'Layout Sample', frame: [100, 100, 500, 400] do
  20.times do
    root.view
  end
  root.view do
    style do 
      self.left = self.top = self.right = self.bottom = '10%'
    end
  end
=begin
  lock_screen = View.new do
    view :header do
      view :left do
        text :dots
        text :carrior
        image :antenna
      end
      image :notifycation_pulltab
      view :right do
        iamge :blutooth
        text :battery_percent
        image :battery
      end
    end

    view :passcode do
      view :main do
      end
      view :footer do
        button :emergency
        button :cancel
      end
    end

    view :slider do
      view :clock
      view :slide
    end
  end
  add lock_screen

  style :all do
    padding 10
  end

  style :header do
    margin 20, 10

    #background gray|graya|rgb|rgba|hsv|hsva,image,bounds,repeat
    #background_color gray|graya|rgb|rgba|hsv|hsva
    #background_image image,bounds,repeat

    #frame x,y,w,h
    #x px|%
    #y px|%
    #width px|%
    #height px|%
    #left px|%
    #top px|%
    #right px|%
    #bottom px|%
  end
=end
end


Reflex.start do
  win.show
end
