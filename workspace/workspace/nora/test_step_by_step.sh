#!/bin/bash

# Test step by step
echo "Step 1: Clean"
echo "clean" | ./ticket_system
echo ""

echo "Step 2: Add user"
echo "add_user -c \"\" -u testuser -p pass123 -n Test -m test@example.com -g 10" | ./ticket_system
echo ""

echo "Step 3: Login"
echo "login -u testuser -p pass123" | ./ticket_system
echo ""

echo "Step 4: Query order"
echo "query_order -u testuser" | ./ticket_system
echo ""

echo "Step 5: Exit"
echo "exit" | ./ticket_system
