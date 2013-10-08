@use_colors = false
if ARGV.include? "-c"
  @use_colors = true
end

PROBS1 = [1, 2, 5, 10, 30, 50, 70, 90, 95, 98, 99]
PROBS2 = [nil]
RANGE = 3..16

reports = []

for j in 0...PROBS2.length
  prob2 = PROBS2[j]
  for i in 0...PROBS1.length
    prob1 = PROBS1[i]
    report = {
      prob: [prob1, prob2],
      results: []
    }

    for i in RANGE
      num = 2**i
      result_array = `./ac.out #{prob1} #{num} #{prob2}`.split("\n").select{|el| (el.include? "Code rate") || (el.include? "Theoric") || (el.include? "WARNING")}
      underflow = false
      correct_bits = 0
      if result_array.select{|el| el.include? "WARNING"}.length > 0
        underflow = true
        correct_bits = result_array.select{|el| el.include? "WARNING"}[0].delete("WARNING - result may be inaccurate: underflow detected at ").delete("th symbol!\n").delete("-").to_i
      else
        result_array = result_array.map{|e| e.split(": ")[1].delete(" bit/symbol")}
      end

      result = {
        num: num,
        underflow: underflow,
        rate: nil,
        entropy: nil,
        distance: nil,
        correct_bits: correct_bits
      }
      if !underflow
        result[:rate] = result_array[0].to_f
        result[:entropy] = result_array[1].to_f
        result[:distance] = result[:rate] - result[:entropy]
      end
      
      report[:results] << result
    end

    reports << report
  end
end

def print_table reports, symbol, invert = false, colors = false, red_major = false
  for i in RANGE
    num = 2**i
    print "\t #{num}"
  end
  print "\n--------|"
  for i in RANGE
    num = 2**i
    print "-------|"
  end
  reports.each do |report|
    print "\n #{report[:prob].select{|e| e != nil}.join(',')}"  
    report[:results].each do |result|
      if !invert
        if result[:underflow]
          print "\t|"
        else
          print "\t|"
          printf result[symbol] < 0 ? color_input("%.3f", result[symbol], colors, red_major) : color_input("%.4f", result[symbol], colors, red_major), result[symbol]
        end
      else
        if !result[:underflow]
          print "\t|"
        else
          printf result[symbol] < 0 ? "\t|%d" : "\t|%d", result[symbol]
        end
      end
    end
    print "\t|\n--------|"
    for i in RANGE
      num = 2**i
      print "-------|"
    end
  end
  puts
end

def color_input(string, value, colors, red_major)
  red = "\033[22;31m"
  green = "\033[22;32m"
  if @use_colors && colors
    if red_major
      string = strcolor string, (value > colors ? red : green)
    else
      string = strcolor string, (value < colors ? red : green)
    end
  end
  string
end

def strcolor string, color
  color + string + "\x1b[0m"
end

puts "Valutazione dell'efficienza del codificatore aritmetico al variare del numero N di bit emessi e della probabilita' P che un simbolo assuma valore 0."
puts "\nRATE DI CODIFICA (le colonne sono il numero di bit emessi, le righe sono la probabilita' che un simbolo sia 0)"
print_table reports, :rate, false, 1, true

puts "\nENTROPIA TEORICA DELLA SORGENTE"
print_table reports, :entropy, false, 0.9, true

puts "\nDIFFERENZA TRA IL RATE DI CODIFICA E L'ENTROPIA TEORICA"
print_table reports, :distance, false, 0

puts "\nNUMERO DI SIMBOLI EMESSI PRIMA DI INCORRERE IN UNDERFLOW"
print_table reports, :correct_bits, true
