import leveldb
db = leveldb.LevelDB("/tmp/test-leveldb")
db.Put("hello", "world")
db.Put("idiom 1", "Whoever sheds the blood of man, by man shall his blood be shed, for God made man in his own image.")
db.Put("idiom 2", "If a man strikes someone with an iron object so that he dies, he is a murderer; the murderer shall be put to death. Or if anyone has a stone in his hand that could kill, and he strikes someone so that he dies, he is a murderer; the murderer shall be put to death. Or if anyone has a wooden object in his hand that could kill, and he hits someone so that he dies, he is a murderer; the murderer shall be put to death. The avenger of blood shall put the murderer to death; when he meets him, he shall put him to death. If anyone with malice aforethought shoves another or throws something at him intentionally so that he dies or if in hostility he hits him with his fist so that he dies, that person shall be put to death; he is a murderer. The avenger of blood shall put the murderer to death when he meets him.")

db.Put("idiom 3", "Human life is sacred,because from its beginning it involves the creative action of God and it remains for ever in a special relationship with the Creator, who is its sole end. God alone is the Lord of life from its beginning until its end: no one can under any circumstance claim for himself the right directly to destroy an innocent human being.The deliberate murder of an innocent person is gravely contrary to the dignity of the human being, to the golden rule, and to the holiness of the Creator. The law forbidding it is universally valid: it obliges each and everyone, always and everywhere... The fifth commandment forbids direct and intentional killing as gravely sinful. The murderer and those who cooperate voluntarily in murder commit a sin that cries out to heaven for vengeance.")

db.Put("idiom 4", "Scripture notes a twofold equity on which this commandment is founded. Man is both the image of God and our flesh. Wherefore, if we would not violate the image of God, we must hold the person of man sacred, if we would not divest ourselves of humanity we must cherish our own flesh. The practical inference to be drawn from the redemption and gift of Christ will be elsewhere considered. The Lord has been pleased to direct our attention to these two natural considerations as inducements to watch over our neighbour's preservation, viz., to revere the divine image impressed upon him, and embraceour own flesh. To be clear of the crime of murder, it is not enough to refrain from shedding man's blood. If in act you perpetrate, if in endeavour you plot,if in wish and design you conceive what is adverse to another's safety, you have the guilt of murder. On the other hand, if you do not according to your means and opportunity study to defend his safety, by that inhumanity you violate the law. But if the safety of the body is so carefully provided for, we may hence infer how much care and exertion is due to the safety of the soul, which is of immeasurably higher value in the sight of God.")

print "hello:"
print db.Get("hello")
print "\n"

print "idiom 1:"
print db.Get("idiom 1")
print "\n"

print "idiom 2:"
print db.Get("idiom 2")
print "\n"

print "idiom 3:"
print db.Get("idiom 3")
print "\n"

print "idiom 4:"
print db.Get("idiom 4")
print "\n\n\n"

print "Delete idiom 3"
db.Delete("idiom 3")
print "printing idiom 3:"
print db.Get("idiom 3")

print db.Property("leveldb.stats")
