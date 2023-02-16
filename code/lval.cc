int main(int a)
{
	int c[24][24][24];
	c[0][1][1]=2;


	int x;
	int y;
	int z;
	int i;
	i=0;
	x=0;
	y=0;
	z=0;

	while(x<256)
	{
		while(y<256)
		{
			while(z<256)
			{

				c[x][y][z]=i;

				int p=c[x][y][z];

				ccassert(p==i);

				i=i+1;

				z=z+1;
			}
			y=y+1;
		}
		x=x+1;
	}

	return c[0][0][1];
}