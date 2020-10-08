.set noreorder

.section .text

# Arguments
#	a0 - source address
#	a1 - number to be added
#	a2 - size of buffer
.global memadd
.type memadd, @function
memadd:
	move	$v0, $a0
	addu	$a0, $a2
	addiu	$a0, -1
.Lloop:
	blez	$a2, .Lexit
	addi	$a2, -1
	lb		$v1, 0($a0)
	add		$v0, $v1, $a1
	sb		$v0, 0($a0)
	addiu	$a0, -1
	b		.Lloop
	nop
.Lexit:
	jr		$ra
	nop
