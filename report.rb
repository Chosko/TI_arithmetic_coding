PROBS = [1, 2, 5, 10, 25, 50, 75, 90, 95, 98, 99]
RANGE = 3..16

reports = []

for i in 0...PROBS.length
  prob = PROBS[i]
  report = {
    prob: prob,
    results: []
  }

  for i in RANGE
    num = 2**i
    result_array = `./ac.out #{prob} #{num}`.split("\n").select{|el| (el.include? "Code rate") || (el.include? "Theoric") || (el.include? "WARNING")}
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

def print_table reports, symbol, invert = false
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
    print "\n #{report[:prob]}"  
    report[:results].each do |result|
      if !invert
        if result[:underflow]
          print "\t|"
        else
          printf result[symbol] < 0 ? "\t|%.3f" : "\t|%.4f", result[symbol]
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

puts "Valutazione dell'efficienza del codificatore aritmetico al variare del numero N di bit emessi e della probabilita' P che un simbolo assuma valore 0."
puts "\nRATE DI CODIFICA (le colonne sono il numero di bit emessi, le righe sono la probabilita' che un simbolo sia 0)"
print_table reports, :rate

puts "\nENTROPIA TEORICA DELLA SORGENTE"
print_table reports, :entropy

puts "\nDIFFERENZA TRA IL RATE DI CODIFICA E L'ENTROPIA TEORICA"
print_table reports, :distance

puts "\nNUMERO DI SIMBOLI EMESSI PRIMA DI INCORRERE IN UNDERFLOW"
print_table reports, :correct_bits, true