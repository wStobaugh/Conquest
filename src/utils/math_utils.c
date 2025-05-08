// Helper function to clamp a value between min and max
int clamp(int val, int min, int max) {
    return (val < min) ? min : (val > max) ? max : val;
}