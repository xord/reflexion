# -*- coding: utf-8 -*-


%w[xot rays reflex]
  .map  {|s| File.expand_path "../../../#{s}/lib", __FILE__}
  .each {|s| $:.unshift s if !$:.include?(s) && File.directory?(s)}

require 'reflex'


Reflex::Application.start name: 'Visuals' do |app|
  Reflex::Window.new title: app.name, frame: [100, 100, 600, 500] do |win|
    win.painter.background = 0.8
    10.times do |i|
      Reflex::TextView.new name: 'hello', data: 'HELLO WORLD!' do |v|
        v.move_to *([i * 5] * 2)
        win.add v
      end
    end
  end.show
end
