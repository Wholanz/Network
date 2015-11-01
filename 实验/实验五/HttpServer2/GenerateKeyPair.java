/**
 * Created by LK on 2015/6/16.
 */
/**
 * Created by LK on 2015/6/15.
 */
import java.security.KeyPair;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.util.Random;

public class GenerateKeyPair {

    private String priKey;
    private String pubKey;

    private PublicKey pubkey;
    private PrivateKey prikey;

    public PublicKey getPubKey() {

        return pubkey;
    }

    public PrivateKey getPriKey() {

        return prikey;
    }

    public GenerateKeyPair() {
        try {
            java.security.KeyPairGenerator keygen = java.security.KeyPairGenerator
                    .getInstance("RSA");
            SecureRandom secrand = new SecureRandom();
//            secrand.setSeed("21cn".getBytes()); // 初始化随机产生器
            Random random = new Random();
            secrand.setSeed(random.nextLong());
            keygen.initialize(512, secrand);
            KeyPair keys = keygen.genKeyPair();

            pubkey = keys.getPublic();
            prikey = keys.getPrivate();

            pubKey = bytesToHexStr(pubkey.getEncoded());
            priKey = bytesToHexStr(prikey.getEncoded());

            System.out.println("pubKey=" + pubKey.toString());
            System.out.println("priKey=" + priKey.toString());


            System.out.println("写入对象 pubkeys ok");
            System.out.println("生成密钥对成功");

//            SignatureData sigData = new SignatureData();
//            VerifySignature veriData = new VerifySignature();
//
//            byte[] signed = sigData.run(prikey, "hehe1");
//            veriData.run(pubkey, signed, "hehe");


        } catch (Exception e) {
            e.printStackTrace();
            System.out.println("生成密钥对失败");
        }


    }

    /**
     * Transform the specified byte into a Hex String form.
     */
    public static final String bytesToHexStr(byte[] bcd) {
        StringBuffer s = new StringBuffer(bcd.length * 2);

        for (int i = 0; i < bcd.length; i++) {
            s.append(bcdLookup[(bcd[i] >>> 4) & 0x0f]);
            s.append(bcdLookup[bcd[i] & 0x0f]);
        }

        return s.toString();
    }

    /**
     * Transform the specified Hex String into a byte array.
     */
    public static final byte[] hexStrToBytes(String s) {
        byte[] bytes;

        bytes = new byte[s.length() / 2];

        for (int i = 0; i < bytes.length; i++) {
            bytes[i] = (byte) Integer.parseInt(s.substring(2 * i, 2 * i + 2),
                    16);
        }

        return bytes;
    }

    private static final char[] bcdLookup = {'0', '1', '2', '3', '4', '5',
            '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

}