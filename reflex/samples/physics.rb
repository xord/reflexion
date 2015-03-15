# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'


Reflex.start name: "Physics" do |app|
  Reflex::Window.show title: app.name, frame: [100, 100, 500, 500] do
    root.set gravity: Reflex::Point.new(0, 9.8), debug: true

    add_dynamic = -> size = 50, x = rand(10..400), y = rand(10..100) do
      add Reflex::EllipseShape.new {
        set :fill, [:red, :green, :blue, :yellow, :cyan, :magenta, :gray].sample
        set :pos, [x, y]
        set :size, [rand(5..size)] * 2
        body.set dynamic: true, density: 1
      }
    end

    add_static = -> size = 50 do
      add Reflex::RectShape.new {
        set :fill, :white
        set :pos, [rand(10..400), rand(200..400)]
        set :size, [rand(5..(size * 2)), rand(5..size)]
        body.set static: true
      }
    end

    50.times {|n| add_dynamic.call}
    5.times {|n| add_static.call}

    after :draw do |e|
      e.painter.fill :white
      e.painter.text "#{e.fps.to_i} FPS", 10, 10
    end

    on :pointer do |e|
      if e.down? || e.drag?
        add_dynamic.call 50, *e.pos.to_a
      end
    end
  end
end
